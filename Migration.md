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

