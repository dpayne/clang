//===- unittest/Format/FormatTestAccess.cpp - Formatting unit tests
//-------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/Format/Format.h"

#include "../Tooling/RewriterTestContext.h"
#include "FormatTestUtils.h"

#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MemoryBuffer.h"
#include "gtest/gtest.h"

#define DEBUG_TYPE "format-test"

namespace clang {
namespace format {
namespace {

class FormatTestClassIndent : public ::testing::Test {
protected:
  std::string format(llvm::StringRef Code, const FormatStyle &Style) {
    LLVM_DEBUG(llvm::errs() << "---\n");
    LLVM_DEBUG(llvm::errs() << Code << "\n\n");
    std::vector<tooling::Range> Ranges(1, tooling::Range(0, Code.size()));
    bool IncompleteFormat = false;
    tooling::Replacements Replaces =
        reformat(Style, Code, Ranges, "<stdin>", &IncompleteFormat);
    EXPECT_FALSE(IncompleteFormat) << Code << "\n\n";
    auto Result = applyAllReplacements(Code, Replaces);
    EXPECT_TRUE(static_cast<bool>(Result));
    LLVM_DEBUG(llvm::errs() << "\n" << *Result << "\n\n");
    return *Result;
  }

  FormatStyle getStyleWithAdditionalIndent() {
    FormatStyle Style = getLLVMStyle();
    Style.AdditionalIndentClassBlock = true;
    return Style;
  }

  void verifyFormat(llvm::StringRef Code, const FormatStyle & Style = getLLVMStyle()) {
    EXPECT_EQ(Code.str(), format(test::messUp(Code), Style));
  }
};

TEST_F(FormatTestClassIndent, NoChangeWithoutAccess) {
  verifyFormat("class A {\n"
               "  int a1;\n"
               "};");
  verifyFormat("class B {\n"
               "    int b1;\n"
               "};",
               getStyleWithAdditionalIndent());
}

TEST_F(FormatTestClassIndent, ChangeWithAccess) {
  verifyFormat("class C {\n"
               "  int c1;\n"
               "public:\n"
               "  int c2;\n"
               "};");
  verifyFormat("class D {\n"
               "    int d1;\n"
               "  public:\n"
               "    int d2;\n"
               "};",
               getStyleWithAdditionalIndent());
}

TEST_F(FormatTestClassIndent, MultipleAccessLevels) {
  verifyFormat("class E {\n"
               "  int e1;\n"
               "public:\n"
               "  int e2;\n"
               "private:\n"
               "  int e3;\n"
               "protected:\n"
               "  int e4;\n"
               "};");
  verifyFormat("class F {\n"
               "    int f1;\n"
               "  public:\n"
               "    int f2;\n"
               "  private:\n"
               "    int f3;\n"
               "  protected:\n"
               "    int f4;\n"
               "};",
               getStyleWithAdditionalIndent());
}

TEST_F(FormatTestClassIndent, NestedClass) {
  verifyFormat("class G {\n"
               "  int g1;\n"
               "  class GGA {\n"
               "    int gg1;\n"
               "  public:\n"
               "    int gg2;\n"
               "  };\n"
               "public:\n"
               "  class GGB {\n"
               "    int gg1;\n"
               "  public:\n"
               "    int gg2;\n"
               "  };\n"
               "  int g2;\n"
               "private:\n"
               "  int g3;\n"
               "protected:\n"
               "  int g4;\n"
               "};");
  verifyFormat("class H {\n"
               "    int h1;\n"
               "    class HHA {\n"
               "        int hh1;\n"
               "      public:\n"
               "        int hh2;\n"
               "    };\n"
               "  public:\n"
               "    class HHB {\n"
               "        int hh1;\n"
               "      public:\n"
               "        int hh2;\n"
               "    };\n"
               "    int h2;\n"
               "  private:\n"
               "    int h3;\n"
               "  protected:\n"
               "    int h4;\n"
               "};",
               getStyleWithAdditionalIndent());
}

} // end namespace
} // end namespace format
} // end namespace clang
