# Contributing

Feel free to make pull requests for contributions to Voxelio. I would love if some people were interested enough to contribute, but be warned, my code is not crazy well optimized.

I only ask that you try to adhere to my coding style. (However, I myself am sometimes slightly incosistent throughout.)

## Style

### Class Member vs Static Variables

Class member variables should be written in the form:

    m_VariableName

While static member variables should be:

    s_VariableName

### Curly Braces

I generally prefer curly braces to be like so:

    void TestFunction()
    {
        // Stuff
    }

However in certain circumstances it can look nicer to do:

    if(testBool) { return test; }

### Function Names

Functions should generally be written in PascalCase:

    void TestFunction()

### Local Variables

Local variables which are not apart of any class should be written in lowerCamelCase:

    int testVar = 0;
