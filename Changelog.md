## Ignition Fuel Tools 4.x

### Ignition Fuel Tools 4.X.X (20xx-xx-xx)

1. Added `edit` subcommand to the `ign fuel`. The edit command currently
   supports editing a model's privacy.
    * [Pull request 67](https://github.com/ignitionrobotics/ign-fuel-tools/pull/67)

1. Alphabetical listing of subcommands.
    * [Pull request 65](https://github.com/ignitionrobotics/ign-fuel-tools/pull/65)

### Ignition Fuel Tools 4.1.0 (2020-02-27)

1. Resource deletion CLI.
    * [BitBucket pull request 119](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/119)

1. Fetch files
    * [BitBucket pull request 123](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/123)

1. README and tutorial updates:
    * [BitBucket pull request 113](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/113)
    * [BitBucket pull request 114](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/114)
    * [BitBucket pull request 115](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/115)

### Ignition Fuel Tools 4.0.0 (2019-12-10)

1. Model upload CLI, resource metadata CLI, depend on ign-msgs5.
    * [BitBucket pull request 108](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/108)

## Ignition Fuel Tools 3.x

### Ignition Fuel Tools 3.x.x (20xx-xx-xx)

### Ignition Fuel Tools 3.2.2 (2020-05-18)

1. Fix URL encodings in RestClient.
    * [Pull request 70](https://github.com/ignitionrobotics/ign-fuel-tools/pull/70)

1. Print message when downloading a resource.
    * [BitBucket pull request 102](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/102)

1. Use `${YAML_TARGET}` instead of `YAML::YAML` imported target.
    * [BitBucket pull request 103](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/103)

1. Convert emissive map file path.
    * [BitBucket pull request 105](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/105)

1. Fix windows build with `popen` and `pclose` macros.
    * [BitBucket pull request 109](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/109)

1. Remove std::experimental for filesystem (support for VS2019).
    * [BitBucket pull request 120](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/120)

### Ignition Fuel Tools 3.2.1 (2019-08-12)

1. Support actors
    * [BitBucket pull request 101](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/101)

### Ignition Fuel Tools 3.2.0 (2019-06-14)

1. FuelClient::DownloadModel accepts HTTP headers, and the `ign fuel
   download` command can accept a single HTTP header.
    * [BitBucket pull request 100](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/100)

### Ignition Fuel Tools 3.1.0 (2019-05-xx)

1. Fix PBR material URI
    * [BitBucket pull request 95](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/95)
    * [BitBucket pull request 96](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/96)

### Ignition Fuel Tools 3.0.0 (2018-02-28)

1. Integration with ign-gazebo. The following set of pull requests update
   fuel-tools to use ign-cmake2, sets api.ignitionfuel.org as the default
   server, fixes codecheck errors, removes old deprecations, and parses
   model.config files using tinyxml2.
    * [BitBucket pull request 82](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/82)
    * [BitBucket pull request 83](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/83)
    * [BitBucket pull request 84](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/84)
    * [BitBucket pull request 85](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/85)
    * [BitBucket pull request 87](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/87)
    * [BitBucket pull request 88](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/88)
    * [BitBucket pull request 89](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/89)

## Ignition Fuel Tools 1.x

### Ignition Fuel Tools 1.x.x (20xx-xx-xx)

1. Use `${YAML_TARGET}` instead of `YAML::YAML` imported target.
    * [BitBucket pull request 103](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/103)

### Ignition Fuel Tools 1.2.0 (2018-05-30)

1. Get cached model resource file
    * [BitBucket pull request 63](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/63)

1. Add some AsString functions
    * [BitBucket pull request 54](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/54)

1. Change cache directory structure and use it
    * [BitBucket pull request 57](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/57)
    * [BitBucket pull request 71](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/71)
    * [BitBucket pull request 73](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/73)

1. Client return only relevant cached models
    * [BitBucket pull request 62](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/62)

1. Model version support
    * [BitBucket pull request 65](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/65)

1. Remove server API version from model unique name
    * [BitBucket pull request 47](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/47)

1. Client handle unique names and get server info from config
    * [BitBucket pull request 55](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/55)

1. Changed model list to use headers for paging instead of iterating until
   a 404 is hit. Also added a mechanism to set the user agent
    * [BitBucket pull request 46](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/46)
    * [BitBucket pull request 61](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/61)

1. FuelClient: Don't use ServerConfig if there's ModelId
    * [BitBucket pull request 56](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/56)

1. List models from command line
    * [BitBucket pull request 43](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/43)
    * [BitBucket pull request 48](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/48)
    * [BitBucket pull request 44](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/44)
    * [BitBucket pull request 45](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/45)

1. Get cached model resource file
    * [BitBucket pull request 63](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/63)

1. Added const constructors for ModelIter and Model
    * [BitBucket pull request 42](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/42)

1. Use common::URI for server URL
    * [BitBucket pull request 59](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/59)

1. Remove server local name
    * [BitBucket pull request 58](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/58)

1. Encode url path before downloading models
    * [BitBucket pull request 41](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/41)

1. Download model from command line
    * [BitBucket pull request 68](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/68)

1. Backport code style changes
    * [BitBucket pull request 69](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/69)
    * [BitBucket pull request 67](https://osrf-migration.github.io/ignition-gh-pages/#!/ignitionrobotics/ign-fuel-tools/pull-requests/67)

### Ignition Fuel Tools 1.0.0 (2018-01-25)
