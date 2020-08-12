# Database_Normalizer

Work in Progress.

Note: A lot of this code is FAR from clean (I was focused on an internship project while making this, so I didn't exactly follow my usual programming practices). I'm currently working on some other projects. However, when I eventually cycle back to this repo, I'll clean this up.

## Project Description

This is a C++ library. Currently, it finds all possible dependencies in a SQLite database (all that consistently hold up). Logic for cleaning up some of these dependencies, including removing extraneous dependencies (i.e., it removes a, b -> c if given b -> c). Most logic for finding a minimum cover is also implemented, but not entirely complete yet.

There are method stubs for normalizing the database into Boyce Codd and Third Normal Form, if possible. These algorithms are the intended final features, so that any SQLite database can be normalized without any knowledge of databse normalization. Currently, my (repo owner) attention is turned to another project, so I likely won't be able to finish this anytime soon; I will hopefully get back to it ASAP.

## Use

For all listed use cases, assume the following code has been run.
```c
   char* filename = "test.db";
   std::string tablename = std::string("COMPANY");
   normalizer normalizationTool = normalizer(filename, tablename);
```
   
1. Find all possible dependencies.

  normalizationTool.find_dependencies()

2. Manually set dependencies.

```c
   std::vector<functional_dependency> functionalDependencies;
   std::vector<std::string> determinant;
   std::vector<std::string> dependant;
   determinant.push_back("ID");
   determinant.push_back("NAME");
   dependant.push_back("AGE");
   dependant.push_back("ID");
   dependant.push_back("ADDRESS");
   dependant.push_back("NAME");
   functionalDependencies.push_back(functional_dependency(determinant, dependant));
   
   std::vector<std::string> schema;
   schema.push_back("ID");
   schema.push_back("AGE");
   schema.push_back("ADDRESS");
   schema.push_back("NAME");
   schema.push_back("SALARY");

   test.set_dependencies(functional_dependencies, schema);
```
3. Find a Minimum cover
```c
  test.find_minimum_cover(schema);
```

