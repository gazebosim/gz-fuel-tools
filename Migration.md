## Ignition Fuel Tools 1.X to 2.X

### Modifications

1. * All setters (functions that set class variables) have been prefixed
   with `Set` and existing functions deprecated. Acronyms within class and
   function names have been changed from all-caps (e.g URL) to camel-case
   (eg. Url).
    * [Pull request #49](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/49)
    * [Pull request #51](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/51)

1. ResultTypes have moved from a plain `enum` inside the `Result` class to
   an `enum class ResultType` outside the `Result` class scope. 
    * [Pull request #51](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-requests/51/update-result-style/diff#chg-include/ignition/fuel_tools/Result.hh)
