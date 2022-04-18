#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <Python.h>
#include <stdint.h>

#define DEVICE_NAME0 "/dev/rpmsg_pru30"
#define DEVICE_NAME1 "/dev/rpmsg_pru31"
#define MAX_BUFFER_SIZE 512

#define VP	4.1     // ns/m

char readBuf[2][MAX_BUFFER_SIZE];

static PyObject *pulsar_prus(PyObject *self, PyObject *args) {
    uint32_t NumberOfInstructions[2];
	int PropagationTime_ns[2];
    float Distance_m[2] = {0, 0};
	uint8_t rd_stat, wr_stat[2];
        
    struct pollfd pfd[2];
    PyObject* py_distances = PyList_New(2);

    pfd[0].fd = open(DEVICE_NAME0, O_RDWR);
 	pfd[1].fd = open(DEVICE_NAME1, O_RDWR);

    
    if (pfd[0].fd < 0) {
        close(pfd[0].fd);
        PyErr_SetString(PyExc_IOError, "Cannot communicate with PRU0");
        return NULL;
    }
    wr_stat[0] = write(pfd[0].fd, "-", 2) == -1;
    if (wr_stat[0]) {
        close(pfd[0].fd);
        PyErr_SetString(PyExc_IOError, "Cannot write to PRU0");
        return NULL;
    }

    if (pfd[1].fd < 0) {
        close(pfd[1].fd);
        PyErr_SetString(PyExc_IOError, "Cannot communicate with PRU1");
        return NULL;
    }
    wr_stat[1] = write(pfd[1].fd, "-", 2) == -1;
    if (wr_stat[1]) {
        close(pfd[1].fd);
        PyErr_SetString(PyExc_IOError, "Cannot write to PRU1");
        return NULL;
    }


    pfd[0].events = POLLIN;
    pfd[1].events = POLLIN;
    rd_stat = poll(pfd, 2, 3000);

    for(int i=0; i<2; i++){
		if((pfd[i].revents & POLLIN) == 0){
            //timeout, reinicializa pru
            close(pfd[i].fd);
            if(i){
				system("echo stop > /sys/class/remoteproc/remoteproc2/state");
				system("echo start > /sys/class/remoteproc/remoteproc2/state");
			}else{
				system("echo stop > /sys/class/remoteproc/remoteproc1/state");
				system("echo start > /sys/class/remoteproc/remoteproc1/state");
			}
		}else{
            read(pfd[i].fd, readBuf[i], MAX_BUFFER_SIZE);
            NumberOfInstructions[i] = (readBuf[i][3] << 24) | (readBuf[i][2] << 16) | (readBuf[i][1] << 8) | readBuf[i][0];
		    PropagationTime_ns[i] = (int) NumberOfInstructions[i]*5;
            PropagationTime_ns[i] -= 35;                //compensacao de atraso de hardware
            Distance_m[i] = PropagationTime_ns[i]/(2*VP);
        }        

        PyObject* py_distance = Py_BuildValue("f", Distance_m[i]);
        PyList_SetItem(py_distances, i, py_distance);
        
		close(pfd[i].fd);
    }

    return py_distances;
}


static PyMethodDef pulseMethod[] = {
    {"pulsar_prus", pulsar_prus, METH_VARARGS, "Python interface for leakage detector's pulse function"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef LLDet_module = {
    PyModuleDef_HEAD_INIT,
    "LLDet",
    "Python interface for leakage detector's pulse function",
    -1,
    pulseMethod
};

PyMODINIT_FUNC PyInit_LLDet(void) {
    return PyModule_Create(&LLDet_module);
}

