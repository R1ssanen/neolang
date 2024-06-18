## Neolang Grammar & Syntax
- Production rules

$$\begin{align}
[\text{root}] &\to [\overrightarrow{\text{stmt}}]\\
[\text{stmt}] &\to \begin{cases}
    exit\space\text{[expr]};\\
    \text{type:}\space\text{ident} = \text{[expr]};\\
    \text{type:}\space var\space\text{ident} = \text{[expr]};\\
\end{cases} \\
[\text{expr}] &\to \begin{cases}
    \text{num\\_lit}\\
    \text{str\\_lit}\\
    \text{ident}
\end{cases}
\end{align}$$
