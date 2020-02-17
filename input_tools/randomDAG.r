
library(pcalg)

set.seed(256)
myDAG <- randomDAG(n = 10, prob= 0.1, lB = 0, uB = 1)

x <- edgeL(myDAG)
for(i in 1:length(x))
    cat(length(x[[i]]$edges), x[[i]]$edges, "\n")

zz <- file("all.Rout", open = "wt")
sink(zz)
sink(zz, type = "message")