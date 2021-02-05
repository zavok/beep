#include <stdlib.h>
#include <stdio.h>
#include <sndio.h>
#include <err.h>

int
mkblock(struct sio_par *Sp, char **b)
{
	char *nb;
	int i, length, bsize;
	length = Sp->rate/440;
	bsize = length * Sp->pchan * Sp->bps;
	nb = malloc(bsize);
	for (i = 0; i < length; i+= Sp->pchan * Sp->bps){
		nb[i] = i > length / 2 ? 0: 0xff;
	}
	*b = nb;
	return bsize;
}

int
main()
{
	struct sio_hdl *S;
	struct sio_par Sp;
	size_t blsize;
	char *block;
	int i;
	S = sio_open(SIO_DEVANY, SIO_PLAY, 0);
	if (S == 0) err(1, "can't open audio device");
	sio_initpar(&Sp);
	Sp.bits  = 8;
	Sp.bps   = 1;
	Sp.sig   = 0;
	Sp.le    = 1;
	Sp.pchan = 1;
	Sp.rate  = 44100;
	Sp.xrun  = SIO_IGNORE;
	if (!sio_setpar(S, &Sp)) err(1, "can't set sio_par.\n");
	if (!sio_getpar(S, &Sp)) err(1, "can't get audio device params.\n");
	if(!sio_start(S)) err(1, "sio_start failed.\n");
	blsize = mkblock(&Sp, &block);
	if (blsize <= 0) err(1, "block size = %d\n", blsize);
	for (i = 0; i < 100; i++) sio_write(S, block, blsize);
	sio_stop(S);
	sio_close(S);
	free(block);
	return 0;
}

