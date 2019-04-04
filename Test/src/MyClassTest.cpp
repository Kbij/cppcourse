/*
** MyClassTest.cpp for CppCourse in /home/qbus/cppcourse/Test/src
**
** Made by Koen Bijttebier
** Login   <>
**
** Started on  Thu Apr 4 10:16:22 AM 2019 Koen Bijttebier
** Last update Fri Apr 4 10:19:02 AM 2019 Koen Bijttebier
*/

#include "MyClass.h"
#include <gtest/gtest.h>

TEST(MyClassTest, Constructor)
{
    MyClass* obj = new MyClass;
    EXPECT_EQ(5, obj->sum());
    delete obj;
}


TEST(MyClassTest, FailingTest)
{
    MyClass* obj = new MyClass;
    EXPECT_EQ(5, obj->sum());
    delete obj;
}