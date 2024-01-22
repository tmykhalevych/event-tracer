#include <error_assert_impl.hpp>
#include <gtest/gtest.h>
#include <inplace_function.hpp>

using namespace event_tracer;

TEST(InplaceFunction, DefaultConstructible)
{
    InplaceFunction<void()> f;
    EXPECT_FALSE(f);
}

TEST(InplaceFunction, NullptrConstructible)
{
    InplaceFunction<void()> f(nullptr);
    EXPECT_FALSE(f);
}

TEST(InplaceFunction, BadInvoke)
{
    InplaceFunction<void()> f;
    f();
    EXPECT_ASSERT();
}

TEST(InplaceFunction, CopyConstructible)
{
    {
        InplaceFunction<void()> src;
        EXPECT_FALSE(src);
        InplaceFunction<void()> dst(src);
        EXPECT_FALSE(dst);
        EXPECT_FALSE(src);
    }
    {
        InplaceFunction<void()> src([] {});
        EXPECT_TRUE(src);
        InplaceFunction<void()> dst(src);
        EXPECT_TRUE(dst);
        EXPECT_TRUE(src);
    }
    {
        InplaceFunction<void()> src([] {});
        EXPECT_TRUE(src);
        InplaceFunction<void(), DEFAULT_CALLABLE_SIZE * 2> dst(src);
        EXPECT_TRUE(dst);
        EXPECT_TRUE(src);
    }
}

TEST(InplaceFunction, MoveConstructible)
{
    {
        InplaceFunction<void()> src;
        EXPECT_FALSE(src);
        InplaceFunction<void()> dst(std::move(src));
        EXPECT_FALSE(dst);
        EXPECT_FALSE(src);
    }
    {
        InplaceFunction<void()> src([] {});
        EXPECT_TRUE(src);
        InplaceFunction<void()> dst(std::move(src));
        EXPECT_TRUE(dst);
        EXPECT_FALSE(src);
    }
    {
        InplaceFunction<void()> src([] {});
        EXPECT_TRUE(src);
        InplaceFunction<void(), DEFAULT_CALLABLE_SIZE * 2> dst(std::move(src));
        EXPECT_TRUE(dst);
        EXPECT_FALSE(src);
    }
}

TEST(InplaceFunction, NullptrAssignable)
{
    InplaceFunction<void()> f([] {});
    EXPECT_TRUE(f);

    f = nullptr;
    EXPECT_FALSE(f);
}

TEST(InplaceFunction, NullptrComparable)
{
    {
        InplaceFunction<void()> f;
        EXPECT_TRUE(f == nullptr);
        EXPECT_FALSE(f != nullptr);
    }
    {
        InplaceFunction<void()> f([] {});
        EXPECT_FALSE(f == nullptr);
        EXPECT_TRUE(f != nullptr);
    }
}

TEST(InplaceFunction, Assigment)
{
    {
        InplaceFunction<void()> src([] {});
        InplaceFunction<void()> dst([] {});
        EXPECT_TRUE(src);
        EXPECT_TRUE(dst);

        src = dst;
        EXPECT_TRUE(src);
        EXPECT_TRUE(dst);
    }
    {
        InplaceFunction<void()> src([] {});
        InplaceFunction<void()> dst([] {});
        EXPECT_TRUE(src);
        EXPECT_TRUE(dst);

        src = std::move(dst);
        EXPECT_TRUE(src);
        EXPECT_FALSE(dst);
    }
}

TEST(InplaceFunction, Invocation)
{
    {
        bool invoked = false;
        InplaceFunction<void()> f = [&invoked] { invoked = true; };
        EXPECT_FALSE(invoked);

        f();
        EXPECT_TRUE(invoked);
    }
    {
        bool invoked = false;
        InplaceFunction<void()> src = [&invoked] { invoked = true; };
        EXPECT_FALSE(invoked);

        InplaceFunction<void()> dst = src;
        EXPECT_FALSE(invoked);

        dst();
        EXPECT_TRUE(invoked);
    }
    {
        bool invoked = false;
        InplaceFunction<void()> src = [&invoked] { invoked = true; };
        EXPECT_FALSE(invoked);

        InplaceFunction<void()> dst = std::move(src);
        EXPECT_FALSE(invoked);

        dst();
        EXPECT_TRUE(invoked);
    }
    {
        bool invoked = false;
        InplaceFunction<void()> src = [&invoked] { invoked = true; };
        EXPECT_FALSE(invoked);

        InplaceFunction<void()> dst(src);
        EXPECT_FALSE(invoked);

        dst();
        EXPECT_TRUE(invoked);
    }
    {
        bool invoked = false;
        InplaceFunction<void()> src = [&invoked] { invoked = true; };
        EXPECT_FALSE(invoked);

        InplaceFunction<void()> dst(std::move(src));
        EXPECT_FALSE(invoked);

        dst();
        EXPECT_TRUE(invoked);
    }
}
