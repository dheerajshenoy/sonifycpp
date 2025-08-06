#include "argparse.hpp"
#include "sol/sol.hpp"
#include "sonify.hpp"

namespace
{
    void init_args(argparse::ArgumentParser &program) noexcept
    {
        program.add_argument("-i", "--input").help("input file");
    }

} // namespace

int
main(int argc, char *argv[])
{
    argparse::ArgumentParser program("sonifycpp");
    init_args(program);
    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &e)
    {
        qFatal() << "Error parsing arguments: " << e.what() << "\n";
        return 1;
    }

    QApplication a(argc, argv);
    Sonify s;
    s.args(program);
    a.exec();
    return 0;
}
