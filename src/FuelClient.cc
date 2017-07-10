#include <ignition/fuel-tools/FuelClient.hh>


namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


/// \brief Private Implementation
class ignft::FuelClientPrivate
{
};


//////////////////////////////////////////////////
FuelClient::FuelClient(const ClientConfig &_config)
{
  // TODO populate dataPtr
}

//////////////////////////////////////////////////
FuelClient::~FuelClient()
{
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models()
{
  // TODO Return ModelIter set to download all models
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models(const ModelIdentifier &_id)
{
  // Todo Return ModelIter set to fetch models matching ModelIdentifier
}

//////////////////////////////////////////////////
Result FuelClient::UploadModel(const std::string &_pathToModelDir,
    const ModelIdentifier &_id)
{
  // TODO Upload a model and return an Result
}

//////////////////////////////////////////////////
Result FuelClient::DeleteModel(const ModelIdentifier &_id)
{
  // TODO Delete a model and return a Result
}
