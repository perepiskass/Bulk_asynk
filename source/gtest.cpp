#include <gtest/gtest.h>
#include "version_lib.h"
// #include "data.h"

// std::condition_variable cv_file;
// std::condition_variable cv_cmd;
// std::mutex mtx_cmd;
// std::mutex mtx_file;
// bool cmd_run;

size_t checkArg(int, char**);


// Тест кейсы на проверку версий
TEST(version_test_case, version_test_patch)
{
    ASSERT_GE(version_patch(), 1);
}
TEST(version_test_case, version_test_minor)
{
    EXPECT_EQ(version_minor(), 1);
}
TEST(version_test_case, version_test_major)
{
    EXPECT_EQ(version_major(), 1);
}

// Тест кейсы на проверку класса для хранения входных данных
// TEST(data_input_test_case, set_data_test)
// {
//     auto bulkPtr = std::shared_ptr<DataIn> {new DataIn(1)};
//     std::shared_ptr<DataToConsole> console{new DataToConsole(bulkPtr)};
//     std::shared_ptr<DataToFile> file{new DataToFile(bulkPtr)};

//     std::thread log([console](){console->update(1);});
//     std::thread file1([file](){file->update(2);});
//     log.detach();
//     file1.detach();
//     cmd_run = false;
//     mtx_cmd.lock();
//     mtx_file.lock();
//     bulkPtr->setData("hello");
//     bulkPtr->setData(" world");

//     EXPECT_EQ(bulkPtr->bulkQ.size(),2);
//     mtx_cmd.unlock();
//     mtx_file.unlock();
// }

TEST(args_test_case, bulk_size_test)
{
    auto st = "99";
    char* size[2]{const_cast<char*>("0"),const_cast<char*>("0")};
    size[1] = const_cast<char*>(st);
    auto bulk_size = checkArg(2, size);

    EXPECT_EQ(bulk_size, 99);
}

// TEST(version_args_case, version_test)
// {
//     auto st = "-version";
//     char* size[2]{const_cast<char*>("0"),const_cast<char*>("0")};
//     size[1] = const_cast<char*>(st);
//     testing::internal::CaptureStdout();
//     checkArg(2, size);
//     // std::cout << size[1];
//     std::string output = testing::internal::GetCapturedStdout();
//     EXPECT_EQ(output, "-version");
// }


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}