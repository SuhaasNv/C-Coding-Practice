#ifndef CREDENTIAL_STORE_ICREDENTIALSTORE_H
#define CREDENTIAL_STORE_ICREDENTIALSTORE_H

#include <optional>
#include <string>
#include <vector>

struct Credential {
    std::string site;
    std::string username;
    std::string password;
};

class ICredentialStore {
public:
    virtual ~ICredentialStore() = default;

    [[nodiscard]] virtual bool AddCredential(const Credential& credential) = 0;
    [[nodiscard]] virtual std::optional<Credential> GetCredentialBySite(
        const std::string& site) const = 0;
    [[nodiscard]] virtual bool RemoveCredential(const std::string& site) = 0;
    [[nodiscard]] virtual bool UpdateCredential(const Credential& credential) = 0;
    [[nodiscard]] virtual std::vector<std::string> ListSites() const = 0;
};

#endif  // CREDENTIAL_STORE_ICREDENTIALSTORE_H
