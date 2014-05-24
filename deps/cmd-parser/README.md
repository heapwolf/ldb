#`cmd-parser`, a lightweight header-only option parser


###Why?

Because why not? I figured this was a good exercise in edge cases.

###Usage

```c++
optionparser::parser p;

p.add_option("--number", "-n")	.help("pass a double")
								.mode(optionparser::store_value);

p.add_option("-file")   .help("pass a list of files to load.")
						.mode(optionparser::store_mult_values)
						.required(true);

p.add_option("-save", "-s") .help("pass a file to save.")
							.mode(optionparser::store_value);

p.eat_arguments(argc, argv);

double number_passed;

if(p.get_value("number"))
{
	number_passed = p.get_value<double>("number");
}

if (p.get_value("file"))
{
    auto filenames = p.get_value<std::vector<std::string>>("file");
}
```



