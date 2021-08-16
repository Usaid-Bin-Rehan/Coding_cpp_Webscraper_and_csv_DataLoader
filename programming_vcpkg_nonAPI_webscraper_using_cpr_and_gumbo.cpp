/*
Dependencies Information:

1. VSCode
2. vcpkg (https://vcpkg.io/en/getting-started.html)
3. cpr is a C/C++ library for HTTP requests, built as a wrapper for the classic cURL and inspired by the Python requests library.
4. gumbo is an HTML parser entirely written in C, which provides wrappers for multiple programming languages, including C++.

Installing Dependencies:

1. Set an environment variable (https://www.twilio.com/blog/2017/01/how-to-set-environment-variables.html)
2. Open any terminal and run: 
	> vcpkg install cpr
	> vcpkg install gumbo
	> vcpkg integrate install 
*/

# include <iostream>
# include <fstream>
# include "cpr/cpr.h"
# include "gumbo.h"

void search_for_links(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        if (node->v.element.tag == GUMBO_TAG_A)
		{
    		GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
    		if (href)
    		{
        		std::string link = href->value;
    		    if (link.rfind("/wiki") == 0)
        	    writeCsv << "article," << link << "\n";
        		else if (link.rfind("#cite") == 0)
        	    writeCsv << "cite," << link << "\n";
        		else
        	    writeCsv << "other," << link << "\n";
    		}
		}
    }

    GumboVector* children = &node -> v.element.children;
    for (unsigned int i=0; i < children->length ;i++)
    {
        search_for_links(static_cast<GumboNode*>(children->data[i]));
    }
}

void search_for_title(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
	}

    if (node->v.element.tag == GUMBO_TAG_H1)
    {
        GumboNode* title_text = static_cast <GumboNode*> (node->v.element.children.data[0]);
        std::cout << title_text->v.text.text << endl;
        return;
    }

    GumboVector* children = &node->v.element.children;
    
    for (unsigned int i = 0; i < children->length; i++)
    {
        search_for_title(static_cast <GumboNode*> (children->data[i]));
    }
}

std::string extract_html_page(void)
{
    cpr::Url url = cpr::Url{"https://en.wikipedia.org/wiki/Poppy_seed_defence"};
    cpr::Response response = cpr::Get(url);
    return response.text;
}

int main()
{	
    std::string page_content = extract_html_page(); /* Making HTTP request to website to retrieve HTML and storing in page_content */
        
    GumboOutput* parsed_response = gumbo_parse(page_content.c_str()); /* Convert previous page_content to HTML tree */

    search_for_title(parsed_response -> root); /* Traverse the HTML tree to look for <h1> using recursion and display */
    
    gumbo_destroy_output(&kGumboDefaultOptions, parsed_response); /* Free the allocated memory */	
	
	
	std::ofstream writeCsv("links.csv"); /* Creating csv file */
	
	writeCsv << "type,link" << endl;
	
	search_for_links(parsed_response -> root); /* Traverse the HTML tree to look for <a> using recursion and write to csv */
	
	writeCsv.close();
}
