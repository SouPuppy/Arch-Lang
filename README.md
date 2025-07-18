<div align="center">
  <picture>
    <img alt="Arch Programming Language: A language that's all about type"
         src="https://raw.githubusercontent.com/SouPuppy/Arch-Docs/refs/heads/master/static/images/logo.svg"
         width="50%">
  </picture>

[Website][Arch] | [Documentation] | [Contributing]
</div>

This is the main source code repository for [Arch].

[Arch]: https://soupup.online/
[Documentation]: https://soupup.online/
[Contributing]: CONTRIBUTING.md

## Theory

Arch explores multiple theories of types and computation. Each theoretical system is implemented in a **dedicated Git branch**. You can switch between them to explore different semantics, syntax, and evaluation strategies.

| Branch            | Description                           | Status       |
|-------------------|---------------------------------------|--------------|
| `theory/dev`      | Experimental branch for prototyping   | in progress  |
| `theory/stlc`     | Simple Typed Lambda Calculus          | in progress  |
| `theory/hott`     | Homotopy Type Theory                  | in progress  |

To try out a particular theory, check out the corresponding branch:

```bash
git checkout theory/<theory-name>
```

## Installation

Read the [Installation] section from the [Documentation]

[Installation]: https://soupup.online/Installation

## References

**Type Theory**

- [HoTT Book]

[HoTT Book]: https://homotopytypetheory.org/book/

## Source Code

> Building from source: [Building Arch]

The Arch source code is organized as follows:

| Directory | Contents                      |
|-----------|-------------------------------|
| `src/`    | source for Arch language core |

[Building Arch]: https://soupup.online/Installation

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).
