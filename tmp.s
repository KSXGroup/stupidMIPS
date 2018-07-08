.text
main:
	li $9, 20
	li $10, 10
	sub $11, $9, $10
	add $8, $10, $11
	div $12, $8, 2
	li $v0, 17
	syscall
