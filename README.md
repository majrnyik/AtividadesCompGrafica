# Atividade 2

 Para a atividade 2 de computação gráfica foi implementando um jogo baseado nos códigos das aulas passadas. O objetivo do jogo é guiar uma espaço-nave 
usando as teclas 'cima, baixo, lado direito, lado esquerdo' de modo que ela não colida com nenhum asteróide. 
 Caso a nave colida o jogo termina com uma tela escrito 'Game Over' até que o jogador aperte outra tecla para reinicar o jogo.

***

## Movimento dos asteróides 
 A sensação de movimento da nave se dá ao fato de que quem se move são os asteróides, ou seja, eles se movem no sentido oposto ao input dado.

***

## Movimento da nave
 Foi implementação a rotação da nave para que desse a impressão de mudança de direção da nave em relação aos asteróides. 

***

## Campo infinito de asteróides
 Assim que um asteróide sai do campo de visão proposto pelo tamanho da janela, ele é movido para o outro extremo, p.ex se ele atinge 
x=20, ele é movido para x =-20.