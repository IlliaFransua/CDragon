#include <gtest/gtest.h>
#include "template_renderer.h"
#include <string>
#include <iostream>

using std::string;
using std::cout, std::endl;

TEST(TemplateRenderer, All) {
    const string path = "../src/views/test_template.html";

    unordered_map<string, vector<string>> data = {
            {"pageName", {"Главная страница"}},
            {"newsList", {"<h1>Новость 1</h1>", "<h2>Новость 2</h2>", "<h3>Новость 3</h3>"}}
        };
        
    const string html = R"(<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Главная страница</title></head><body><h1>hello world      </h1><a>куксиси: </a><h1>Новость 1</h1><a>куксиси: </a><h2>Новость 2</h2><a>куксиси: </a><h3>Новость 3</h3><h1>hello world</h1></body></html>)";
    
    EXPECT_EQ(TemplateRenderer::render(path, data), html);
}