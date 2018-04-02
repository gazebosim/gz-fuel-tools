## Ignition Fuel Tools 1.X to 2.X

### Modifications

1. * All setters (functions that set class variables) have been prefixed
   with `Set` and existing functions deprecated. Acronyms within class and
   function names have been changed from all-caps (e.g URL) to camel-case
   (eg. Url).
    * [Pull request #49](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/49)
    * [Pull request #51](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/51)
    * [Pull request #52](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/52)

1. Deprecated `env` function. Please use `igition::common::env`.
    * [Pull request #50](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/50)

1. Deprecated the `ModelIdentifier::Category` functions. The Category concept does not exist on fuelserver.
    * [Pull request #52](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/52)
 
1. Deprecated the accessor `ModelIdentifier::Likes` for `ModelIdentifier::LikeCount`, and the mutator `ModelIdentifier::Likes` for `ModelIdentifier::SetLikeCount`.
    * [Pull request #52](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/52)

1. Deprecated the accessor `ModelIdentifier::Downloads` for `ModelIdentifier::DownloadCount`, and the mutator `ModelIdentifier::Downloads` for `ModelIdentifier::SetDownloadCount`.
    * [Pull request #52](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/52)


1. ResultTypes have moved from a plain `enum` inside the `Result` class to
   an `enum class ResultType` outside the `Result` class scope. 
    * [Pull request #51](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/51/update-result-style/diff#chg-include/ignition/fuel_tools/Result.hh)

1. `ResultType Result::Type() const` now returns an `enum class`
   instead of a plain `enum`. This should not affect you unless you have
   been mapping `ResultType` to an `int`.
    * [Pull request #51](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/51/update-result-style/diff#chg-include/ignition/fuel_tools/Result.hh)
