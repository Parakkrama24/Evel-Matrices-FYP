void calculate_average_anonymity_degree()
{
    // This variable accumulates the total anonymity values (Σ df)
    double anonymity_sum = 0.0;

    // Counts how many flows require privacy (|Fψ|)
    uint32_t privacy_flow_counter = 0;

    // Loop through all flows in the system
    for (uint32_t fid = 0; fid < 2 * flows; fid++)
    {
        // ψf → privacy demand of flow f (assigned by LLM)
        double psi_f = flow_privacy_value[fid];

        // τψ → privacy threshold (only flows above this use privacy mode)
        double tau_psi = 0.5;

        // Check if this flow requires privacy (ψf > τψ)
        // This corresponds to selecting flows in set Fψ
        if (psi_f > tau_psi)
        {
            // df → achieved anonymity degree of flow f
            // (calculated earlier using Eq. 3.51)
            double d_f = anonymity_degree[fid];

            // Add anonymity value to total (Σ df)
            anonymity_sum += d_f;

            // Count this flow as a privacy-mode flow
            privacy_flow_counter++;
        }
    }

    // Compute average anonymity for the CURRENT routing cycle
    // d̄ = (1 / |Fψ|) * Σ df
    if (privacy_flow_counter > 0)
        current_anonymity_degree = anonymity_sum / privacy_flow_counter;
    else
        // If no privacy flows exist, anonymity is 0
        current_anonymity_degree = 0.0;

    // Accumulate anonymity values across cycles
    // This is used to compute overall average over time
    previous_cumulative_anonymity_degree += current_anonymity_degree;

    // Final average across all routing cycles so far
    // d̄_total = (1 / K) * Σ d̄_k
    average_anonymity_degree =
        previous_cumulative_anonymity_degree / data_gathering_cycle_number;

    // Print result for monitoring/debugging
    cout << "average anonymity degree is "
         << average_anonymity_degree << endl;
}

void calculate_privacy_mode_overhead()
{
    // This variable accumulates total extra delay caused by privacy (Σ ΔTf)
    double overhead_sum = 0.0;

    // Counts number of flows that require privacy (|Fψ|)
    uint32_t privacy_flow_counter = 0;

    // Loop through all flows in the system
    for (uint32_t fid = 0; fid < 2 * flows; fid++)
    {
        // ψf → privacy demand of flow f (assigned by LLM)
        double psi_f = flow_privacy_value[fid];

        // τψ → privacy threshold (only flows above this use privacy mode)
        double tau_psi = 0.5;

        // Check if this flow requires privacy (ψf > τψ)
        // This selects flows belonging to Fψ
        if (psi_f > tau_psi)
        {
            // Latency when privacy mode is enabled (garlic routing)
            double privacy_latency = privacy_mode_latency[fid];

            // Latency under normal routing (no privacy)
            double normal_latency  = standard_routing_latency[fid];

            // ΔTf → additional delay caused by privacy
            double delta_T = privacy_latency - normal_latency;

            // Safety check: ensure overhead is not negative
            // (privacy should not reduce latency; if it does, ignore it)
            if (delta_T < 0.0)
                delta_T = 0.0;

            // Add this flow’s overhead to total (Σ ΔTf)
            overhead_sum += delta_T;

            // Count this as a privacy-mode flow
            privacy_flow_counter++;
        }
    }

    // Compute average privacy overhead for the CURRENT routing cycle
    // ΔT = (1 / |Fψ|) * Σ ΔTf
    if (privacy_flow_counter > 0)
        current_privacy_overhead = overhead_sum / privacy_flow_counter;
    else
        // If no privacy flows exist, overhead is zero
        current_privacy_overhead = 0.0;

    // Accumulate overhead values across cycles
    previous_cumulative_privacy_overhead += current_privacy_overhead;

    // Final average across all routing cycles so far
    // ΔT_total = (1 / K) * Σ ΔT_k
    average_privacy_overhead =
        previous_cumulative_privacy_overhead / data_gathering_cycle_number;

    // Print result (converted to milliseconds for readability)
    cout << "average privacy overhead is "
         << 1000.0 * average_privacy_overhead << " ms" << endl;
}

void calculate_fl_convergence_round()
{
    // εFL → convergence threshold (small value means models must be very similar)
    double epsilon_FL = 1e-3;

    // This will track the maximum deviation across all clusters
    double max_deviation = 0.0;

    // Loop through all clusters in the system
    for (uint32_t k = 0; k < number_of_clusters; k++)
    {
        // Calculate how different cluster k's model is from the global average
        // (i.e., || θk - θ̄ || )
        double deviation = calculate_parameter_deviation(k);

        // Keep the largest deviation (worst-case cluster)
        if (deviation > max_deviation)
            max_deviation = deviation;
    }

    // Check if all cluster models are close enough (i.e., converged)
    // Condition:
    // 1. max deviation ≤ threshold → models are similar
    // 2. fl_convergence_round == 0 → convergence not recorded yet
    if (max_deviation <= epsilon_FL && fl_convergence_round == 0)
    {
        // Record the current gossip round as convergence round
        // This stores the FIRST round when convergence occurs
        fl_convergence_round = current_gossip_round;
    }

    // Store convergence round for current state (used for output/analysis)
    current_fl_convergence_round = fl_convergence_round;

    // Print result for monitoring/debugging
    cout << "FL convergence round is "
         << current_fl_convergence_round << endl;
}