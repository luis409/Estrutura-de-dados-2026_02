library(ggplot2)

dados <- read.csv("resultados.csv")

grafico <- ggplot(dados, aes(x = n, y = media, colour = estrutura)) +
  geom_line(linewidth = 1) +
  geom_point(size = 2) +
  scale_colour_manual(values = c("Lista" = "tomato", "ABB" = "green3")) +
  scale_x_continuous(trans = "log2", breaks = unique(dados$n)) +
  scale_y_log10() +
  labs(
    title    = "Análise de Algoritmos",
    subtitle = "Média de comparações por tamanho da entrada",
    x        = "Tamanho da Entrada (N)",
    y        = "Média de Comparações",
    colour   = "Estrutura"
  ) +
  theme_minimal()

print(grafico)
ggsave("grafico_crescimento.png", plot = grafico, width = 8, height = 5)