#pragma once

#include <iostream>

namespace ettention
{
	namespace inpainting 
	{	
	    class Serializable
        {
        public:
			Serializable();
			virtual ~Serializable();

			template<typename T >
			std::ostream& binary_write(std::ostream& stream, const T& value) {
				return stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
			}

			std::ostream& binary_write(std::ostream& stream, const std::string value)
			{
				unsigned int size = (unsigned int) value.size();
				binary_write(stream, size);
				return stream.write(reinterpret_cast<const char*>(&value[0]), size);
			}

			std::ostream& binary_write(std::ostream& stream, const char* value)
			{
				std::string s(value);
				return binary_write( stream, s );
			}

			template<typename T >
			std::ostream& binary_write(std::ostream& stream, const std::vector<T>& value)
			{
				unsigned int size = (unsigned int) value.size();
				binary_write(stream, size);
				return stream.write(reinterpret_cast<const char*>(&value[0]), size * sizeof(T) );
			}

			template<typename T >
			inline void binary_read(std::istream& stream, T& value) {
				stream.read(reinterpret_cast<char*>(&value), sizeof(T));
			}

			template<typename T >
			inline void binary_read(std::istream& stream, std::vector<T>& value)
			{
				unsigned int size;
				binary_read(stream, size); 
				value.resize(size);
				stream.read(reinterpret_cast<char*>(&value[0]), sizeof(T) * size );
			}

			inline void binary_read(std::istream& stream, std::string& value)
			{
				unsigned int size;
				binary_read(stream, size);
				value.resize(size);
				stream.read(reinterpret_cast<char*>(&value[0]), size);
			}

			inline void binary_lookahead(std::istream& stream, std::string& value)
			{
				auto lastPosition = stream.tellg();
				unsigned int size;
				binary_read(stream, size);
				value.resize(size);
				stream.read(reinterpret_cast<char*>(&value[0]), size);
				stream.seekg(lastPosition);
			}

			inline void ensure_binary_read(std::istream& stream, std::string value)
			{
				std::string buffer;
				unsigned int size;
				binary_read(stream, size);
				value.resize(size);
				stream.read(reinterpret_cast<char*>(&value[0]), size);
				if (buffer != value)
					std::ios_base::failure("file format error expected " + value);
			}


		public:
			virtual void writeToStream( std::ostream& os ) = 0;
			virtual void loadFromStream( std::istream& is ) = 0;

		};

	} // namespace inpainting
} // namespace ettention`