#include "pch.h"
#include "utils/Utils.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World" << std::endl;

    ErrorHandle::WSAErrorMessageBox("Error MessageBox Test");
}