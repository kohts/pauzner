package main;

import "fmt"
import "os"
import "strings"

func main() {
    if len(os.Args) >= 2 && os.Args[1] == "test" {
        fmt.Printf("would run tests here\n")
    } else {
        if len(os.Args) < 2 {
            fmt.Printf("usage: shunting_yard test|EXPRESSION\n");
            os.Exit(0);
        }

        var expression string = "";
        var i int;

        for i = 1; i < len(os.Args) ; i++ {
            expression = strings.Join([]string{expression, os.Args[i]}, "")
        }

        fmt.Printf("got expression: [%s]\n", expression);
    }
}
