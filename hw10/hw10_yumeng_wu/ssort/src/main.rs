extern crate rand;
use rand::Rng;

use std::env;
use std::f32;
use std::fs::{File, OpenOptions};
use std::io::{Cursor, Read, Seek, SeekFrom, Write};
use std::sync::{Arc, Barrier, Mutex};
use std::thread;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 4 {
        println!("Usage: {} <threads> input output", args[0]);
        return;
    }

    let threads = args[1].parse::<usize>().unwrap();
    let inp_path = &args[2];
    let out_path = &args[3];

    // Sample
    // Calculate pivots
    let mut inpf = File::open(inp_path).unwrap();
    let size = read_size(&mut inpf);
    let pivots = find_pivots(&mut inpf, threads);

    // Create output file
    {
        let mut outf = File::create(out_path).unwrap();
        let tmp = size.to_le_bytes();
        outf.write_all(&tmp).unwrap();
        outf.set_len(size).unwrap();
    }

    let mut workers = vec![];

    // Spawn worker threads
    let sizes = Arc::new(Mutex::new(vec![0u64; threads]));
    let barrier = Arc::new(Barrier::new(threads));
    let mutex__ = Arc::new(Mutex::new(0 as u32));

    for ii in 0..threads {
        let inp = inp_path.clone();
        let out = out_path.clone();
        let piv = pivots.clone();
        let szs = sizes.clone();
        let bar = barrier.clone();
        let mm = mutex__.clone();

        let tt = thread::spawn(move || {
            worker(ii, inp, out, piv, szs, bar, mm);
        });
        workers.push(tt);
    }

    // Join worker threads
    for tt in workers {
        tt.join().unwrap();
    }
}

fn read_size(file: &mut File) -> u64 {
    // TODO: Read size field from data file
    (file.metadata().unwrap().len() - 8) / 4
    // 0
}

fn read_item(file: &mut File, ii: u64) -> f32 {
    // TODO: Read the ii'th float from data file
    let mut num_buf = [0; 4];
    file.seek(SeekFrom::Start(8 + ii * 4));
    file.read(&mut num_buf);
    f32::from_le_bytes(num_buf)
}

fn sample(file: &mut File, count: usize, size: u64) -> Vec<f32> {
    let mut rng = rand::thread_rng();
    let mut ys: Vec<f32> = Vec::new();

    // TODO: Sample 'count' random items from the
    // provided file
    let mut i = 0;
    while i < count {
        let idx = rng.gen_range(0, size);
        ys.push(read_item(file, idx));
        i += 1;
    }

    ys
}

fn swap_f32(arr: &mut Vec<f32>, i: usize, j: usize) {
    let temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

fn partition(arr: &mut Vec<f32>, left: usize, right: usize) -> usize {
    let pivot = arr[right];
    let mut i = left.clone().wrapping_sub(1);
    for j in (left.clone()..right.clone()) {
        if arr[j] < pivot {
            i = i.wrapping_add(1);
            swap_f32(arr, i, j);
        }
    }
    swap_f32(arr, i.wrapping_add(1), right.clone());
    i.wrapping_add(1)
}

fn quick_sort(arr: &mut Vec<f32>, l: usize, r: usize) {
    if (l as isize) < (r as isize) {
        let p = partition(arr, l, r);
        quick_sort(arr, l, p.wrapping_sub(1));
        quick_sort(arr, p.wrapping_add(1), r);
    }
}

fn find_pivots(file: &mut File, threads: usize) -> Vec<f32> {
    // TODO: Sample 3*(threads-1) items from the file
    let mut count_buf = [0; 8];
    file.read(&mut count_buf);
    let number = u64::from_le_bytes(count_buf); // total numbers
    let count = 3 * (threads - 1);

    let mut random_samples = sample(file, count, number);

    // TODO: Sort the sampled list
    quick_sort(&mut random_samples, 0, count.wrapping_sub(1));

    let mut pivots = vec![0f32];

    // TODO: push the pivots into the array
    for i in 0..threads.wrapping_sub(1) {
        pivots.push(random_samples[i * 3 + 1]);
    }

    pivots.push(f32::INFINITY);
    pivots
}

fn worker(
    tid: usize,
    inp_path: String,
    out_path: String,
    pivots: Vec<f32>,
    sizes: Arc<Mutex<Vec<u64>>>,
    bb: Arc<Barrier>,
    mm: Arc<Mutex<u32>>,
) {
    // TODO: Open input as local fh
    let mut file = File::open(inp_path).unwrap();

    // TODO: Scan to collect local data
    // let data = vec![0f32, 1f32];
    let mut data: Vec<f32> = Vec::new();
    {
        let mut size_buf = [0; 8];
        file.read(&mut size_buf);
        let number = u64::from_le_bytes(size_buf);
        for i in 0..number {
            let xx = read_item(&mut file, i);
            if xx >= pivots[tid] && xx < pivots[tid.wrapping_add(1)] {
                data.push(xx);
            }
        }
    }

    // TODO: Write local size to shared sizes
    {
        // curly braces to scope our lock guard
        sizes.lock().unwrap()[tid] = data.len() as u64;
    }

    // println!("wait");
    // bb.wait();

    // {
    //     let ssss = sizes.lock().unwrap().len();
    //     for i in 0..ssss {
    //         println!("tid: {}, i: {}, sizes: {}", tid, i, sizes.lock().unwrap()[i]);
    //     }
    // }

    // TODO: Sort local data
    let size = data.len().clone().wrapping_sub(1);
    quick_sort(&mut data, 0, size);

    // Here's our printout
    println!("{}: start {}, count {}", tid, &data[0], data.len());

    // TODO: Write data to local buffer
    let mut cur = Cursor::new(vec![]);

    for xx in &data {
        let tmp = xx.to_ne_bytes();
        cur.write_all(&tmp).unwrap();
    }

    bb.wait();

    // TODO: Get position for output file
    // let prev_count = {
    //     // curly braces to scope our lock guard
    //     5
    // };
    let mut start: u64 = 0;
    {
        let vv = sizes.lock().unwrap();
        for i in 0..tid {
            start += vv[i].clone();
        }
    }
    let mut end: u64 = start;
    {
        end += sizes.lock().unwrap()[tid];
    }

    /*
    let mut outf = OpenOptions::new()
        .read(true)
        .write(true)
        .open(out_path).unwrap();
    */
    // TODO: Seek and write local buffer.
    {
        let mmm = mm.lock().unwrap();
        let mut outf = OpenOptions::new()
            .read(true)
            .write(true)
            .open(out_path)
            .unwrap();
        outf.seek(SeekFrom::Start(8 + start * 4));
        for xx in &data {
            let tmp = xx.to_le_bytes();
            outf.write_all(&tmp).unwrap();
        }
        // let mut bytes = [0; ];
    }

    // TODO: Figure out where the barrier goes.
}
