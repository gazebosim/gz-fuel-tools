set(proto_files
  ${PROJECT_SOURCE_DIR}/proto/gz/msgs/fuel_metadata.proto
  ${PROJECT_SOURCE_DIR}/proto/gz/msgs/version.proto
  ${PROJECT_SOURCE_DIR}/proto/gz/msgs/version_range.proto
  ${PROJECT_SOURCE_DIR}/proto/gz/msgs/versioned_name.proto
)

gz_msgs_generate_messages(
  TARGET gz-fuel_tools9-msgs
  PROTO_PACKAGE "gz.msgs"
  MSGS_PATH ${PROJECT_SOURCE_DIR}/proto
  MSGS_PROTOS ${proto_files}
  DEPENDENCIES gz-msgs10::gz-msgs10-msgs
)

install(
  DIRECTORY gz
  DESTINATION share/protos
  COMPONENT proto
  FILES_MATCHING PATTERN "*.proto")
