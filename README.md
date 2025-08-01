<div align="center">
  <picture>
    <img alt="Arch Programming Language: A language that's all about type"
         src="https://raw.githubusercontent.com/SouPuppy/Arch-Docs/refs/heads/master/static/images/logo.svg"
         width="50%">
  </picture>

[Website][Arch] | [Documentation] | [Contributing]
</div>

This is the main source code repository for [Arch].

[Arch]: https://arch-lang.site/
[Documentation]: https://arch-lang.site/
[Contributing]: CONTRIBUTING.md

## Theory

Arch explores multiple theories of types and computation. Each theoretical system is implemented in a **dedicated Git branch**. You can switch between them to explore different semantics, syntax, and evaluation strategies.

| Branch            | Description                           | Status       |
|-------------------|---------------------------------------|--------------|
| `theory/dev`      | Experimental branch for prototyping   | in progress  |
| `theory/utlc`     | Untyped Lambda Calculus               | in progress  |
| `theory/stlc`     | Simple Typed Lambda Calculus          | in progress  |
| `theory/systemf`  | System F (second-order polymorphic λ) | in progress  |
| `theory/mltt`     | Martin-Löf Type Theory                | in progress  |
| `theory/hott`     | Homotopy Type Theory                  | in progress  |

To try out a particular theory, check out the corresponding branch:

```bash
git checkout theory/<theory-name>
```

## Installation

Read the [Installation] section from the [Documentation]

[Installation]: https://arch-lang.site

## References

**Type Theory**

- [HoTT Book]

[HoTT Book]: https://homotopytypetheory.org/book/

## Source Code

The Arch source code is organized as follows:

| Directory | Contents                      |
|-----------|-------------------------------|
| `src/`    | source for Arch language core |

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).
