#include "stdafx.h"

#include "InpaintingFrontEnd.h"

using namespace ettention;
using namespace ettention::inpainting;

void printError(std::string message) noexcept
{
    std::cout << message << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "Exemplar-Based Inpainting" << std::endl;
    try
    {
        InpaintingFrontEnd frontEnd(argc, argv);
        frontEnd.performInpainting();

    } catch( const std::exception& e )
    {
        std::cout << "error: " << e.what() << std::endl;
        printError(e.what());
    }
    catch(...)
    {
        printError("Unknown ERROR");
    }

    return 0;
}