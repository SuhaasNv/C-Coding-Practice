#include "ICredentialStore.h"
#include "InMemoryCredentialStore.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {

void PrintResult(const std::string& test_name, const bool passed) {
    std::cout << (passed ? "[PASS] " : "[FAIL] ") << test_name << '\n';
}

bool Contains(const std::vector<std::string>& items, const std::string& value) {
    for (const auto& item : items) {
        if (item == value) {
            return true;
        }
    }

    return false;
}

}  // namespace

int main() {
    // Interface pointer shows how callers can depend on abstraction, not implementation.
    std::unique_ptr<ICredentialStore> store = std::make_unique<InMemoryCredentialStore>();

    const Credential github{"github.com", "suhaas", "token_123"};
    const Credential edge{"microsoft.com", "edge_user", "edge_password"};

    PrintResult("Add valid credential", store->AddCredential(github));
    PrintResult("Add second credential", store->AddCredential(edge));
    PrintResult("Reject duplicate site", !store->AddCredential(github));

    PrintResult("Reject empty site", !store->AddCredential(Credential{"", "u", "p"}));
    PrintResult("Reject empty username", !store->AddCredential(Credential{"site", "", "p"}));
    PrintResult("Reject empty password", !store->AddCredential(Credential{"site", "u", ""}));

    const auto fetched = store->GetCredentialBySite("github.com");
    PrintResult("Retrieve existing credential", fetched.has_value());
    if (fetched.has_value()) {
        PrintResult("Retrieved username matches", fetched->username == "suhaas");
    }

    PrintResult("Retrieve unknown site returns nullopt",
                !store->GetCredentialBySite("unknown.com").has_value());
    PrintResult("Retrieve empty site returns nullopt",
                !store->GetCredentialBySite("").has_value());

    PrintResult("Update existing credential",
                store->UpdateCredential(Credential{"github.com", "suhaas_updated", "token_999"}));
    const auto updated = store->GetCredentialBySite("github.com");
    PrintResult("Updated value persisted", updated.has_value() && updated->username == "suhaas_updated");

    PrintResult("Reject update for missing site",
                !store->UpdateCredential(Credential{"no-site", "user", "pwd"}));
    PrintResult("Reject update with empty fields",
                !store->UpdateCredential(Credential{"github.com", "", "pwd"}));

    const auto sites = store->ListSites();
    PrintResult("List contains github.com", Contains(sites, "github.com"));
    PrintResult("List contains microsoft.com", Contains(sites, "microsoft.com"));

    PrintResult("Remove existing credential", store->RemoveCredential("microsoft.com"));
    PrintResult("Remove missing credential returns false", !store->RemoveCredential("missing.com"));
    PrintResult("Remove empty site returns false", !store->RemoveCredential(""));

    std::cout << "Final site count: " << store->ListSites().size() << '\n';
    return 0;
}
