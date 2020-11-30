
#include "FbxImporter.h"

#include "PathHelper.h"
#include <iostream>
#include <filesystem>

using namespace inceptionengine;


int main()
{
	std::string enginePath = PathHelper::GetEnginePath(std::filesystem::current_path().string());
	PathHelper::SetEngineDirectory(enginePath);

	std::string filePath;
	std::string outputFolder;
	std::cout << "Importer tool starts. All path required should be absolute or relative to the engine folder\n";
	std::cout << "Enter the path to the fbx file: ";
	std::getline(std::cin, filePath);

	std::cout << "Enter the path to the output folder: ";
	std::getline(std::cin, outputFolder);

	fbximport::Import(filePath, outputFolder);



}