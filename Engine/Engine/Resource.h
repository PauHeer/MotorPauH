#pragma once

#include <string>

enum class ResourceType
{
	TEXTURE,
	MODEL,
	UNKNOWN = -1
};

class Resource
{
public:
	Resource(const std::string& name, ResourceType type) : name(name), type(type) {}
	virtual ~Resource() {}

	ResourceType GetType() const { return type; }
	const std::string& GetName() const { return name; }

	void SetAssetFileDir(const std::string& assetFileDir) { this->assetFileDir = assetFileDir; }
	void SetLibraryFileDir(const std::string& libraryFileDir) { this->libraryFileDir = libraryFileDir; }

	const std::string& GetAssetFileDir() const { return assetFileDir; }
	const std::string& GetLibraryFileDir() const { return libraryFileDir; }

private:
	ResourceType type;
	std::string name;
	std::string assetFileDir;
	std::string libraryFileDir;
};