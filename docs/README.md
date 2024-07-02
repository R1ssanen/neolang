## Neolang Grammar & Syntax

- Production rules

$$\begin{align}
[\text{root}] &\to \vec{[\text{stmt}]} \\
[\text{scope}] &\to \vec{[\text{stmt}]} \\
[\text{stmt}] &\to \begin{cases}
    exit \space \text{[expr];} \\
    \text{[asgn];} \\
    \text{[decl];} \\
    \text{[var\\_def];} \\
    \text{[scope]} \\
    \text{[fun\\_decl];} \\
    \text{[fun\\_def];} \\
    \text{[call];} \\
    for \space \text{[var\\_def]} : \space \text{[expr]} \space \text{[scope]} \\
    if \space \text{[expr]} \space \text{[scope]} \\
    while \space \text{[expr]} \space \text{[scope]} \\
\end{cases} \\
\text{[call]} &\to \text{ident}( \text{[expr]}... )\\
\text{[fun\\_def]} &\to \text{[fun\\_decl]} \space \text{[scope]} \\
\text{[fun\\_decl]} &\to \text{[decl]}( \text{[decl]}... ) \\
\text{[expr]} &\to \begin{cases}
    \text{[term]} \\
    \text{[bin\\_expr]} \\
    \text{[interval]} \\
    \text{[asgn]} \\
    \text{[call]} \\
\end{cases} \\
\text{[bin\\_expr]} &\to \text{[expr]} \space \text{op} \space \text{[expr]} \\
\text{[interval]} &\to [\text{[expr]},\text{[expr]}] \\
\text{[asgn]} &\to \text{ident} = \text{[expr]} \\
\text{[var\\_def]} &\to \text{[decl]} = \text{[expr]} \\
\text{[decl]} &\to \text{type:} \space var^* \space \text{ident} \\
\text{[term]} &\to \begin{cases}
    \text{num\\_lit} \\
    \text{ident} \\
\end{cases} \\
\end{align}$$
