#pragma once

#include <string>
#include "absl/types/optional.h"

namespace vi {

struct Options {
    absl::optional<bool> useSimulcast = true;
    absl::optional<bool> useSharingSimulcast = true;
    absl::optional<bool> forceTcp;
    absl::optional<bool> produce = true;
    absl::optional<bool> consume = true;
    absl::optional<bool> forceH264;
    absl::optional<bool> forceVP9;
    absl::optional<std::string> svc;
    absl::optional<bool> datachannel = true;
    absl::optional<std::string> throttleSecret;
    absl::optional<std::string> e2eKey;
};

}
