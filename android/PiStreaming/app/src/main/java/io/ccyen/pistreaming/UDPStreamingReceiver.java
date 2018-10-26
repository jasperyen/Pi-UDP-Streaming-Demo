package io.ccyen.pistreaming;


import android.util.Log;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

/**
 *
 * @author User
 */
public class UDPStreamingReceiver extends Thread {
    private static final int UPD_BUFFER_SIZE = 65536;
    private static final int MAX_BUFFER_SIZE = 5;
    private static final double ACCEPT_THRESHOLD = 0.95;
    private final Queue<ByteBuffer> dataQueue;
    private final List<BufferStruct> dataBuffer;
    private final DatagramSocket socket;
    private boolean keepRunning = false;
    private int buffCount = 0;
    private int frameCount = 0;


    public class BufferStruct {
        ByteBuffer data;
        int tag, length, count;
    }

    public UDPStreamingReceiver(int bindPort) throws SocketException {
        Log.i("UDPStreamingReceiver", "listen port : " + bindPort);

        socket = new DatagramSocket(bindPort);
        socket.setSoTimeout(1000);

        dataBuffer = new ArrayList<>();
        dataQueue = new LinkedList<>();
        for (int i = 0; i < MAX_BUFFER_SIZE; ++i) {
            dataBuffer.add(new BufferStruct());
            dataBuffer.get(i).data = ByteBuffer.allocate(0);
        }
    }

    public void close() throws InterruptedException {
        keepRunning = false;
        this.join();
    }

    public ByteBuffer getData () {
        synchronized (dataQueue) {
            return dataQueue.poll();
        }
    }

    private void addDataToBuffer(byte[] rawData, int dataLength){
        int packageTag = ByteBuffer.wrap(rawData, 0, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
        int packageLength = ByteBuffer.wrap(rawData, 4, 8).order(ByteOrder.LITTLE_ENDIAN).getInt();
        int packageSize = ByteBuffer.wrap(rawData, 8, 12).order(ByteOrder.LITTLE_ENDIAN).getInt();
        int packageShift = ByteBuffer.wrap(rawData, 12, 16).order(ByteOrder.LITTLE_ENDIAN).getInt();


        BufferStruct buff = null;
        for (BufferStruct b : dataBuffer)
            if (b.tag == packageTag)
                buff = b;


        if (buff == null) {
            buffCount = ++buffCount % MAX_BUFFER_SIZE;
            buff = dataBuffer.get(buffCount);

            if (buff.count < buff.length) {
                if ((double)buff.count / buff.length > ACCEPT_THRESHOLD
                        && buff.count != buff.length) {
                    synchronized (dataQueue) {
                        dataQueue.add(buff.data);
                    }
                    frameCount++;
                    Log.i("addDataToBuffer",
                            "accept package(" + buff.count + "/" + buff.length + ") : " + buff.tag);
                }
                else
                    Log.w("addDataToBuffer",
                            "loss package("+buff.count+"/"+buff.length+") : " + buff.tag);
            }

            buff.count = 0;
            buff.tag = packageTag;
            buff.length = packageLength;

            byte[] tmp = buff.data.array();
            int limit = tmp.length > packageSize ? packageSize : tmp.length;

            buff.data = ByteBuffer.allocate(packageSize);
            buff.data.put(tmp, 0, limit);
        }

        //Log.i("addDataToBuffer", "update package : " + packageTag);

        buff.count++;
        buff.data.position(packageShift);
        buff.data.put(rawData, 16, dataLength - 16);
        buff.data.rewind();

        if (buff.count == buff.length) {
            //Log.d("addDataToBuffer", "package receive complete : " + packageTag);
            synchronized (dataQueue) {
                dataQueue.add(buff.data);
            }
            frameCount++;
        }
    }

    @Override
    public void run() {
        byte[] buf = new byte[UPD_BUFFER_SIZE];
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        int frame = 0, recv = 0;
        double sec = 0;
        long t = 0;

        keepRunning = true;

        while (keepRunning) {
            t = System.nanoTime();
            try {
                t = System.nanoTime();
                socket.receive(packet);
                //Log.i("UDPStreamingReceiver", "receive data : " + packet.getLength());
            } catch (IOException ex) {
                Log.w("UDPStreamingReceiver", ex.toString());
                continue;
            }
            sec += (System.nanoTime() - t) / 1000.0 / 1000.0 / 1000.0;
            recv += packet.getLength();

            if (sec > 1) {
                Log.i("UDPStreamingReceiver", "Network speed : " + (recv / sec / 1024.0)
                        + "KB/s  Frame rate : " + (frameCount - frame) / sec);
                recv = 0; frame = frameCount; sec = 0;
            }

            addDataToBuffer(packet.getData(), packet.getLength());
            packet.setLength(UPD_BUFFER_SIZE);
        }
        socket.close();
    }

}
