## Ignition Fuel Tools 4.x

### Ignition Fuel Tools 4.X.X (20xx-xx-xx)

1. Set license information based on licenses available from a Fuel server
   and `legal` information in a `metadata.pbtxt` file.
    * [Pull request 61](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/61)

### Ignition Fuel Tools 4.1.0 (2020-02-27)

1. Resource deletion CLI.
    * [Pull request 119](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/119)

1. Fetch files
    * [Pull request 123](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/123)

1. README and tutorial updates:
    * [Pull request 113](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/113)
    * [Pull request 114](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/114)
    * [Pull request 115](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/115)

### Ignition Fuel Tools 4.0.0 (2019-12-10)

1. Model upload CLI, resource metadata CLI, depend on ign-msgs5.
    * [Pull request 108](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/108)

## Ignition Fuel Tools 3.x

### Ignition Fuel Tools 3.x.x (20xx-xx-xx)

1. Print message when downloading a resource.
    * [Pull request 102](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/102)

1. Use `${YAML_TARGET}` instead of `YAML::YAML` imported target.
    * [Pull request 103](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/103)

1. Convert emissive map file path.
    * [Pull request 105](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/105)

1. Fix windows build with `popen` and `pclose` macros.
    * [Pull request 109](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/109)

1. Remove std::experimental for filesystem (support for VS2019).
    * [Pull request 120](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/120)

### Ignition Fuel Tools 3.2.1 (2019-08-12)

1. Support actors
    * [Pull request 101](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/101)

### Ignition Fuel Tools 3.2.0 (2019-06-14)

1. FuelClient::DownloadModel accepts HTTP headers, and the `ign fuel
   download` command can accept a single HTTP header.
    * [Pull request 100](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/100)

### Ignition Fuel Tools 3.1.0 (2019-05-xx)

1. Fix PBR material URI
    * [Pull request 95](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/95)
    * [Pull request 96](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/96)

### Ignition Fuel Tools 3.0.0 (2018-02-28)

1. Integration with ign-gazebo. The following set of pull requests update
   fuel-tools to use ign-cmake2, sets api.ignitionfuel.org as the default
   server, fixes codecheck errors, removes old deprecations, and parses
   model.config files using tinyxml2.
    * [Pull request 82](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/82)
    * [Pull request 83](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/83)
    * [Pull request 84](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/84)
    * [Pull request 85](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/85)
    * [Pull request 87](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/87)
    * [Pull request 88](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/88)
    * [Pull request 89](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/89)

## Ignition Fuel Tools 1.x

### Ignition Fuel Tools 1.x.x (20xx-xx-xx)

1. Use `${YAML_TARGET}` instead of `YAML::YAML` imported target.
    * [Pull request 103](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/103)

### Ignition Fuel Tools 1.2.0 (2018-05-30)

1. Get cached model resource file
    * [Pull request 63](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/63)

1. Add some AsString functions
    * [Pull request 54](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/54)

1. Change cache directory structure and use it
    * [Pull request 57](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/57)
    * [Pull request 71](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/71)
    * [Pull request 73](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/73)

1. Client return only relevant cached models
    * [Pull request 62](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/62)

1. Model version support
    * [Pull request 65](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/65)

1. Remove server API version from model unique name
    * [Pull request 47](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/47)

1. Client handle unique names and get server info from config
    * [Pull request 55](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/55)

1. Changed model list to use headers for paging instead of iterating until
   a 404 is hit. Also added a mechanism to set the user agent
    * [Pull request 46](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/46)
    * [Pull request 61](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/61)

1. FuelClient: Don't use ServerConfig if there's ModelId
    * [Pull request 56](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/56)

1. List models from command line
    * [Pull request 43](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/43)
    * [Pull request 48](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/48)
    * [Pull request 44](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/44)
    * [Pull request 45](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/45)

1. Get cached model resource file
    * [Pull request 63](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/63)

1. Added const constructors for ModelIter and Model
    * [Pull request 42](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/42)

1. Use common::URI for server URL
    * [Pull request 59](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/59)

1. Remove server local name
    * [Pull request 58](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/58)

1. Encode url path before downloading models
    * [Pull request 41](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/41)

1. Download model from command line
    * [Pull request 68](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/68)

1. Backport code style changes
    * [Pull request 69](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/69)
    * [Pull request 67](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/67)

### Ignition Fuel Tools 1.0.0 (2018-01-25)
