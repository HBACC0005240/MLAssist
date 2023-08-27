"./protoc.exe" -I=./ --cpp_out=./ --grpc_out=./ --plugin=protoc-gen-grpc="./grpc_cpp_plugin.exe" ./*.proto
"./protoc.exe" -I=./ --python_out=./ --grpc_out=./ --plugin=protoc-gen-grpc="./grpc_python_plugin.exe" ./*.proto
pause