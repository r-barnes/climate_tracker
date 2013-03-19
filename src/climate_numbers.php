<?php include('/home/rick/www/latex/class.latexrender.php');?>
<html>
<body style="background-color:beige">
<h1>Averaging</h1>
<p>My initial thought was to average over a year and than average over the number of years in the climate. Some notes on this.</p>
<p>An average of averages is the same as an overall average, provided the cardinality (number of elements in a set) of each average is the same. However, this is not a strict requirement. A derivation of the fractional error is as follows:</p>
<p>The average of averages is defined as <?php formula('\frac{1}{2}(\frac{X_1}{N_1}+\frac{X_2}{N_2})'); ?>, while the total average is <?php formula('\frac{X_1+X_2}{N_1+N_2}'); ?>.</p>
<p>The fractional difference this then given by <?php formula('\frac{\frac{1}{2}(\frac{X_1}{N_1}+\frac{X_2}{N_2})}{\frac{X_1+X_2}{N_1+N_2}}'); ?>
<p><?php formula('(\frac{X_1}{2N_1}+\frac{X_2}{2N_2})(\frac{N_1+N_2}{X_1+X_2})'); ?></p>
<p><?php formula('(\frac{N_2 X_1+N_1 X_2}{2 N_1 N_2})(\frac{N_1+N_2}{X_1+X_2})'); ?></p>
<p><?php formula('\frac{N_2 N_1 X_1+N_1 N_1 X_2+N_2 N_2 X_1+N_1 N_2 X_2}{2 N_1 N_2 (X_1+X_2)}'); ?></p>
<p><?php formula('\frac{X_1}{2(X_1+X_2)}+\frac{N_1 X_2}{2N_2(X_1+X_2)}+\frac{N_2 X_1}{2N_1(X_1+X_2)}+\frac{X_2}{2(X_1+X_2)}'); ?></p>
<p><?php formula('\frac{X_1+X_2}{2(X_1+X_2)}+\frac{N_1 X_2}{2N_2(X_1+X_2)}+\frac{N_2 X_1}{2N_1(X_1+X_2)}'); ?></p>
<p><?php formula('\frac{1}{2}+\frac{N_1 X_2}{2N_2(X_1+X_2)}+\frac{N_2 X_1}{2N_1(X_1+X_2)}'); ?></p>
<p>The above is reassuring because it tells us that the fractional error can't be greater than one-half.</p>
<p>Now, let <?php formula('X_2=X_1+\Delta X');?> and <?php formula('N_2=N_1+\Delta N');?>, then:</p>
<p><?php formula('\frac{1}{2}+\frac{N_1 (X_1 + \Delta X)}{2(N_1+\Delta N)(X_1+X_1+\Delta X)}+\frac{(N_1+\Delta N) X_1}{2N_1(X_1+X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+\frac{N_1 (X_1 + \Delta X)}{2(N_1+\Delta N)(2X_1+\Delta X)}+\frac{(N_1+\Delta N) X_1}{2N_1(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+\frac{N_1 X_1}{2(N_1+\Delta N)(2X_1+\Delta X)}+\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+\frac{N_1 X_1}{2N_1(2X_1+\Delta X)}+\frac{X_1 \Delta N}{2N_1(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+\frac{N_1 X_1}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 X_1}{2N_1(2X_1+\Delta X)}+
\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 \Delta N}{2N_1(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+\frac{N_1 N_1 X_1+N_1 X_1 (N_1+\Delta N)}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 \Delta N}{2N_1(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+
\frac{N_1 N_1 X_1}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 X_1 (N_1+\Delta N)}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 \Delta N}{2N_1(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+
\frac{N_1 N_1 X_1}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 X_1 N_1}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 X_1 \Delta N}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 \Delta N}{2N_1(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+
\frac{N_1 X_1}{(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 \Delta N}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 \Delta N}{2N_1(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+
\frac{N_1 X_1}{(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 N_1 \Delta N+X_1 \Delta N(N_1+\Delta N)}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+
\frac{N_1 X_1}{(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 N_1 \Delta N+X_1 N_1 \Delta N+X_1 \Delta N \Delta N}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}'); ?></p>
<p><?php formula('\frac{1}{2}+
\frac{N_1 X_1}{(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{N_1 \Delta X}{2(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 \Delta N}{(N_1+\Delta N)(2X_1+\Delta X)}+
\frac{X_1 \Delta N \Delta N}{2N_1(N_1+\Delta N)(2X_1+\Delta X)}'); ?></p>
<hr>
<p>However, it is not regularly the case that the yearly sums are the same because we expect that the climate does change over time. Nor is the yearly cardinality always the same because there are missing data points (leap-years would probably cause only small drifts).</p>
<p>For leap-years the cardinality mismatch causes, if we assume a 40F average temperature, a 0.12F error.</p>
</body>
</html>
