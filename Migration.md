## Ignition Fuel Tools 3.X to 4.X

### Depercations

1. The `fuel.ignitionrobotics.org` Fuel server has been deprecated, and
    redirects to `fuel.gazebosim.org`. Use `fuel.gazebosim.org` in all Fuel
    URLs.

### Modifications

1. `FuelClient::UploadModel` takes 2 more arguments now.
1. `RestClient::Request`'s `_form` argument is now an `std::multimap` instead of `std::map`.

## Ignition Fuel Tools 1.X to 2.X

### Modifications

1. Renamed the REST class to Rest.
    * [BitBucket pull request #53](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/53)

1. All setters (functions that set class variables) have been prefixed
   with `Set` and existing functions deprecated. Acronyms within class and
   function names have been changed from all-caps (e.g URL) to camel-case
   (eg. Url).
    * [BitBucket pull request #49](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/49)
    * [BitBucket pull request #51](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/51)
    * [BitBucket pull request #52](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/52)

1. Deprecated `env` function. Please use `igition::common::env`.
    * [BitBucket pull request #50](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/50)

1. Deprecated the `ModelIdentifier::Category` functions. The Category concept does not exist on fuelserver.
    * [BitBucket pull request #52](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/52)

1. Deprecated the accessor `ModelIdentifier::Likes` for `ModelIdentifier::LikeCount`, and the mutator `ModelIdentifier::Likes` for `ModelIdentifier::SetLikeCount`.
    * [BitBucket pull request #52](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/52)

1. Deprecated the accessor `ModelIdentifier::Downloads` for `ModelIdentifier::DownloadCount`, and the mutator `ModelIdentifier::Downloads` for `ModelIdentifier::SetDownloadCount`.
    * [BitBucket pull request #52](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/52)


1. ResultTypes have moved from a plain `enum` inside the `Result` class to
   an `enum class ResultType` outside the `Result` class scope.
    * [BitBucket pull request #51](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/51/update-result-style/diff#chg-include/ignition/fuel_tools/Result.hh)

1. `ResultType Result::Type() const` now returns an `enum class`
   instead of a plain `enum`. This should not affect you unless you have
   been mapping `ResultType` to an `int`.
    * [BitBucket pull request #51](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/51/update-result-style/diff#chg-include/ignition/fuel_tools/Result.hh)

## Ignition Fuel Tools 1.2 to 1.X

### Modifications



## Ignition Fuel Tools 1.0 to 1.2

### Modifications

1. Model unique name doesn't contain server API version anymore, for example:
    * **Old**: http://localhost:8001/1.0/alice/models/hello
    * **New**: http://localhost:8001/alice/models/hello

1. Servers no longer have a local name: API calls return garbage and config fields are ignored.

1. Cache structure has been changed, for example:

    * **Old**

        /home/user/.ignition/fuel/
        └── models
            └── username
                └── modelname

    * **New**

        /home/user/.ignition/fuel/
        └── serverurl.org
            └── username
                 └── models
                       └── modelname
                             ├── 1
                             └── 2

