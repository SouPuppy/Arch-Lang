**Expressions**

```
DecL ::=  KEYWORD_DEFINE identifier ':' type
      |   KEYWORD_DEFINE identifier ':=' term
      |   KEYWORD_DEFINE identifier ':' type ':=' term

term ::=  (term)
      |   term term
      |   identifier ':' type
      |   builtinTerm
      |   constTerm

type ::= (type)
      |  term
      |  struct { [DecL]+ }
      |  type -> type
      |  baseTerm

builtinTerm ::= baseTerm
      |  '(+)': term: ℝ -> term: ℝ -> term: ℝ
      |  '(×)': term: ℝ -> term: ℝ -> term: ℝ

baseTerm ::=
        term ':' 'String'
      | term ':' 'ℝ'

constString ::= DOUBLE_QUOTE * DOUBLE_QUOTE
      |  (constString)
      |  constString + constString

constReal ::= (constReal)
      |  constReal + constReal
      |  constReal × constReal

```

**Instructions**

```
#type term
#eval term
```