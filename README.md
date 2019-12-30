# FunC Programming Language Compiler

The FunC language has a Lisp-like syntax and compiles to C code using GNU Bison and Flex.

Please use [this VS Code extension](https://marketplace.visualstudio.com/items?itemName=natiiix.func-language-support) for syntax highlighting.

## Example Code

**Obligatory "Hello World!":**

```func
[stdio]

(func main () int
    (puts "Hello World!")
)
```

See the [example FunC code file](example.func), which is used for testing purposes, to get a better idea of what this language looks like.

## License

The entire FunC project is licensed under the [MIT License](LICENSE), allowing everyone free use without any warranty.
