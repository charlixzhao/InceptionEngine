#pragma once

#include <string>

namespace inceptionengine
{
	class PathHelper
	{
	public:
		enum class FileType
		{
			File,
			Directory,
			SoftLink,
			HardLink
		};
		static void Normalize(std::string& path)
		{
			;
		}

		static void SetEngineDirectory(std::string& path)
		{
			mEnginePath = path;
		}

		static bool IsAbsolutePath(std::string const& path)
		{
			return path.length() >= 2 && isupper(path[0]) && path[1] == ':';
		}

		static bool IsFileExist(std::string const& path)
		{
			return true;
		}

		static FileType GetFileType(std::string const& path)
		{
			return FileType::File;
		}

		static std::string GetFileExtension(std::string const& path)
		{
			return "";
		}

		static std::string GetAbsolutePath(std::string const& path)
		{
			if (IsAbsolutePath(path)) return path;
			return mEnginePath + path;
		}

	private:
		/*
		This should be the path of the folder "Inception Engine", 
		Examples are D:\\InceptionEngine 
		*/
		static std::string mEnginePath;  
	};


}