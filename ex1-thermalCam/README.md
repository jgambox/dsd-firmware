## Thermal read image

### Assumption

* the thermal camera generates an array with EOF given by the following BYTES 0xFF 0xD9

### References

* https://www.file-recovery.com/jpg-signature-format.htm
* https://marketplace.visualstudio.com/items?itemName=formulahendry.code-runner

### Code

* The logic is detect this EOF Bytes and break the loop. In this way we know that we reach the end of the hex array.

#### Please check thermalmage.c file or run thermalmage.exe

```

 /*
 * delimiter of thermal array 0xFF, 0xD9
 */
void print_hex(unsigned int* array_list) {
        
    int i = 0;

    for (;;) {
        *array_list = *(array_list + i);

        printf("0x%X, ", *array_list);
        i++;
        
        if (*array_list == 0xff) {
            if (*(array_list + i) == 0xd9)
            {   
                printf("<= ( 0xFF, 0xD9) EOF DETECTED");
                printf("\n");
                printf("\n");
                printf("PRESIONE CUALQUIER TECLA PARA FINALIZAR");
                getchar();  
                break;
            }   
        }       
    }           
}               

int main()
{
    print_hex(thermalImage);
    return 0;
}

```

### Results

* hexadecimal representation of an image

![](https://github.com/jgambox/dsd-firmware/blob/master/ex1-thermalCam/asset/captura_hex_image.PNG)

* Ouput

![](https://github.com/jgambox/dsd-firmware/blob/master/ex1-thermalCam/asset/output_methodRead_img.PNG)


