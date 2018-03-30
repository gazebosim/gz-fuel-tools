## Ignition Fuel Tools 1.X to 2.X

### Modifications

1. * All setters (functions that set class variables) have been prefixed
   with `Set` and existing functions deprecated. Acronyms within class and
   function names have been changed from all-caps (e.g URL) to camel-case
   (eg. Url).
    * [Pull request #49](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/49)
    * [Pull request #52](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/52)

1. Deprecated the `ModelIdentifier::Category` functions. The Category concept does not exist on fuelserver.
    * [Pull request #52](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/pull-request/52)
 
