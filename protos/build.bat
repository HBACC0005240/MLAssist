"./protoc.exe" -I=./ --cpp_out=./ --grpc_out=./ --plugin=protoc-gen-grpc="./grpc_cpp_plugin.exe" ./*.proto
pause