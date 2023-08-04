set(proto_files
  ${PROJECT_SOURCE_DIR}/proto/gz/msgs/fuel_metadata.proto
  ${PROJECT_SOURCE_DIR}/proto/gz/msgs/version.proto
  ${PROJECT_SOURCE_DIR}/proto/gz/msgs/version_range.proto
  ${PROJECT_SOURCE_DIR}/proto/gz/msgs/versioned_name.proto
)

gz_msgs_generate_messages(
  TARGET msgs
  PROTO_PACKAGE "gz.msgs"
  MSGS_PATH ${PROJECT_SOURCE_DIR}/proto
  MSGS_PROTOS ${proto_files}
  DEPENDENCIES gz-msgs${GZ_MSGS_VER}::gz-msgs${GZ_MSGS_VER}-msgs
)

install(
  DIRECTORY gz
  DESTINATION share/protos
  COMPONENT proto
  FILES_MATCHING PATTERN "*.proto")
