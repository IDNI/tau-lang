CID=$(docker create packages)
mkdir -p build-Release
docker cp $CID:/tau-lang/build-Release/packages build-Release
docker rm $CID
