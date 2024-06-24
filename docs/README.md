## Neolang Grammar & Syntax

- Production rules

$$\begin{align}
[\text{root}] &\to [\vec{ \text{stmt} }] \\
[\text{stmt}] &\to \begin{cases}
    put \space \text{str\\_lit} & _{print} \\
    exit \space \text{[expr];} & _{exit \space statement} \\
    \text{type:} \space \text{ident;} & _{declaration} \\
    \text{ident} = \text{[expr];} & _{assignment} \\
    \text{type:} \space \text{ident} = \text{[expr];} & _{definition}
\end{cases} \\
[\text{expr}] &\to \begin{cases}
    \text{[term]} \\
    \text{[bin\\_expr]}
\end{cases} \\
[\text{bin\\_expr}] &\to \begin{cases}
    [\text{expr}] \space \text{op} \space[\text{expr}]
\end{cases} \\
[\text{term}] &\to \begin{cases}
    \text{num\\_lit} \\
    \text{ident}
\end{cases} \\
\end{align}$$
