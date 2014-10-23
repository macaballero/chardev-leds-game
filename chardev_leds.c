/*
*	chardev_leds.c
*
*	Copyright (C) Miguel A. Caballero 
*
*
*	Simple linux module character device to control the leds on your keyboard.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/*Prototipos*/
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_write(struct file *,const char *, size_t, loff_t *);

/*Constantes*/
#define EXITO 0
#define NOMBRE_DISPOSITIVO "leds"
#define RETARDO 1000
#define BUF_LEN 80

/*Variables globales*/
static int Major;
static int Dispositivo_abierto = 0;
static char *msg_Ptr;
static struct file_operations fops = {
	.write = device_write,
	.open = device_open,
	.release = device_release
};

/*Funciones*/
int init_module(void)
{
	Major = register_chrdev(0,NOMBRE_DISPOSITIVO,&fops);
	if(Major < 0)
	{
		printk(KERN_ALERT "Fallo al registar el dispositivo de caracteres con Major %d\n",Major);
		return Major;
	}
	
	printk(KERN_INFO "He sido asignado al Major %d.\n",Major);
	printk(KERN_INFO "Para hablar con el driver, teclea: 'mknod /dev/%s c %d 0'\n",NOMBRE_DISPOSITIVO,Major);
	printk(KERN_INFO "Borrar el archivo del dispositivo y el modulo cuando hayas acabado\n");
	return EXITO;
}

void cleanup_module(void)
{
	unregister_chrdev(Major,NOMBRE_DISPOSITIVO);
}

/*Metodos*/

static int device_open(struct inode *inode, struct file *file)
{
	if (Dispositivo_abierto)
		return -EBUSY;
	Dispositivo_abierto++;
	try_module_get(THIS_MODULE);
	return EXITO;
}

static int device_release(struct inode *inode, struct file *file)
{
	Dispositivo_abierto--;
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_write(struct file *filp,const char *buff, size_t len, loff_t *off)
{
	int intentos=5,bits=0,i=0,state=0x00;
	char message[len];
	if(copy_from_user(message,buff,len) !=0)
	{
		printk(KERN_ALERT "Error al copiar el buffer desde el entorno de usuario\n"); 
 		return -EINVAL;
	}

	outb(0xed,0x60);//Avisamos al teclado para cambiar los leds
	udelay(RETARDO);
	while(intentos != 0 && inb(0x60)!=0xFA)//Mientras no nos diga que el teclado esta preparado
	{
		intentos--;
		udelay(RETARDO);
	}

	if(intentos!=0)//Si el teclado ha contestado que esta listo
	{
		for(bits; bits < len; bits++)
		{
			switch(message[bits])
			{
				case '1':/*Cambiar bit 1*/
				state = state | (0x1 << 1);
				break;
				case '2':/*Cambiar bit 2*/
				state = state | (0x1 << 2);
				break;
				case '3':/*Cambiar bit 0*/
				state = state | (0x1 << 0);
				break;
				default:
				break;
			}
		}
		outb(state,0x60);
		printk("state vale %d\n",state);
	}
	else
		printk(KERN_ALERT "Se ha excedido el tiempo de espera y el teclado no estaba preparado");
	
	return bits;
}
