#ifndef CREDENTIAL_STORE_INMEMORYCREDENTIALSTORE_H
#define CREDENTIAL_STORE_INMEMORYCREDENTIALSTORE_H

#include "ICredentialStore.h"

#include <string>
#include <unordered_map>
#include <vector>

class InMemoryCredentialStore final : public ICredentialStore {
public:
    explicit InMemoryCredentialStore() = default;
    ~InMemoryCredentialStore() override = default;

    InMemoryCredentialStore(const InMemoryCredentialStore&) = delete;
    InMemoryCredentialStore& operator=(const InMemoryCredentialStore&) = delete;
    InMemoryCredentialStore(InMemoryCredentialStore&&) noexcept = default;
    InMemoryCredentialStore& operator=(InMemoryCredentialStore&&) noexcept = default;

    [[nodiscard]] bool AddCredential(const Credential& credential) override;
    [[nodiscard]] std::optional<Credential> GetCredentialBySite(
        const std::string& site) const override;
    [[nodiscard]] bool RemoveCredential(const std::string& site) override;
    [[nodiscard]] bool UpdateCredential(const Credential& credential) override;
    [[nodiscard]] std::vector<std::string> ListSites() const override;

private:
    static bool IsValidCredentialInput(const Credential& credential) noexcept;
    static bool IsValidSiteInput(const std::string& site) noexcept;

    std::unordered_map<std::string, Credential> credentials_by_site_;
};

#endif  // CREDENTIAL_STORE_INMEMORYCREDENTIALSTORE_H
