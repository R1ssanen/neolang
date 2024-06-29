## Neolang Grammar & Syntax

- Production rules

$$\begin{align}
[\text{root}] &\to \vec{[\text{stmt}]} \\
[\text{stmt}] &\to \begin{cases}
    \text{[exit]}; \\
    \text{ident} = \text{[expr];} \\
    \text{[decl];} \\
    \text{[var\\_def];} \\
    \text{for} \space \text{[var\\_def];} \space \text{[interval]} \space \text{[scope]} \\
    \text{if} \space \text{[expr]} \space \text{[scope]} \\
    [\text{scope}] \\
\end{cases} \\
[\text{interval}] &\to [\text{[expr]}..\text{[expr]}] \\
[\text{var\\_def}] &\to \text{[decl]} = \text{[expr]} \\
[\text{fun\\_decl}] &\to \text{[decl]}(\text{[var\_decl]...}) \\
[\text{decl}] &\to \text{type:} \space var^* \space \text{ident} \\
[\text{scope}] &\to \vec{[\text{stmt}]} \\
[\text{exit}] &\to exit \space \text{[expr]} \\
[\text{expr}] &\to \begin{cases}
    \text{[term]} \\
    \text{[bin\\_expr]} \\
\end{cases} \\
[\text{bin\\_expr}] &\to \begin{cases}
    [\text{expr}] \space \text{op} \space[\text{expr}] \\
\end{cases} \\
[\text{term}] &\to \begin{cases}
    \text{num\\_lit} \\
    \text{ident} \\
\end{cases} \\
\end{align}$$
