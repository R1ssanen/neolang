## Neolang Grammar & Syntax

- Production rules

$$\begin{align}
[\text{root}] &\to [\vec{\text{stmt}}]\\
[\text{stmt}] &\to \begin{cases}
    exit\space\text{[expr];} & _{exit\space statement}\\
    \text{type:}\space\text{ident;} & _{declaration}\\
    \text{ident} = \text{[expr];} & _{assignment}\\
    \text{type:}\space\text{ident} = \text{[expr];} & _{definition}
\end{cases} \\
[\text{expr}] &\to \begin{cases}
 \text{[term]}
\end{cases}\\
[\text{term}] &\to \begin{cases}
 \text{num\\_lit}\\
 \text{ident}
\end{cases}
\end{align}$$
