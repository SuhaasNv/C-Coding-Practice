#include "InMemoryCredentialStore.h"

#include <algorithm>

bool InMemoryCredentialStore::AddCredential(const Credential& credential) {
    if (!IsValidCredentialInput(credential)) {
        return false;
    }

    const auto [_, inserted] = credentials_by_site_.emplace(credential.site, credential);
    return inserted;
}

std::optional<Credential> InMemoryCredentialStore::GetCredentialBySite(
    const std::string& site) const {
    if (!IsValidSiteInput(site)) {
        return std::nullopt;
    }

    const auto iterator = credentials_by_site_.find(site);
    if (iterator == credentials_by_site_.end()) {
        return std::nullopt;
    }

    return iterator->second;
}

bool InMemoryCredentialStore::RemoveCredential(const std::string& site) {
    if (!IsValidSiteInput(site)) {
        return false;
    }

    return credentials_by_site_.erase(site) > 0;
}

bool InMemoryCredentialStore::UpdateCredential(const Credential& credential) {
    if (!IsValidCredentialInput(credential)) {
        return false;
    }

    const auto iterator = credentials_by_site_.find(credential.site);
    if (iterator == credentials_by_site_.end()) {
        return false;
    }

    iterator->second = credential;
    return true;
}

std::vector<std::string> InMemoryCredentialStore::ListSites() const {
    std::vector<std::string> sites;
    sites.reserve(credentials_by_site_.size());

    for (const auto& [site, _] : credentials_by_site_) {
        sites.push_back(site);
    }

    std::sort(sites.begin(), sites.end());
    return sites;
}

bool InMemoryCredentialStore::IsValidCredentialInput(const Credential& credential) noexcept {
    return !credential.site.empty() && !credential.username.empty() && !credential.password.empty();
}

bool InMemoryCredentialStore::IsValidSiteInput(const std::string& site) noexcept {
    return !site.empty();
}
